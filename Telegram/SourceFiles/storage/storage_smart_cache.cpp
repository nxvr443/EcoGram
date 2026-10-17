/*
This file is part of Telegram Desktop fork.
Smart Cache Manager implementation.
*/
#include "storage/storage_smart_cache.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QCryptographicHash>
#include <unordered_map>
#include <vector>
#include <crl/crl_async.h>
#include "base/timer.h"

namespace Storage {

SmartCache &SmartCache::Instance() {
	static SmartCache instance;
	return instance;
}

void SmartCache::startBackgroundMaintenance(const QString &userBasePath) {
	if (_maintenanceRunning || userBasePath.isEmpty()) {
		return;
	}
	_maintenanceRunning = true;

	crl::async([this, userBasePath] {
		const auto cacheDir = userBasePath + "cache";
		const auto mediaCacheDir = userBasePath + "media_cache";

		runOptimizationPass(cacheDir, _maxChannelCacheSize);
		runOptimizationPass(mediaCacheDir, _maxChannelCacheSize);

		_maintenanceRunning = false;
	});
}

SmartCache::Stats SmartCache::runOptimizationPass(const QString &cachePath, int64 targetLimitBytes) {
	Stats stats;
	QDir dir(cachePath);
	if (!dir.exists()) {
		return stats;
	}

	const auto entries = dir.entryInfoList(QDir::Files, QDir::Time | QDir::Reversed);
	stats.totalFiles = entries.size();

	std::unordered_map<QByteArray, QString> seenHashes;
	std::vector<QFileInfo> pruneCandidates;

	for (const auto &info : entries) {
		const auto sz = info.size();
		stats.totalBytes += sz;

		// Skip maps, settings, or tiny metadata files
		if (info.fileName().startsWith("map") || info.fileName().startsWith("data#") || sz < 4096) {
			continue;
		}

		// Quick deduplication via SHA-256 on small/medium files
		if (sz <= 16 * 1024 * 1024) {
			QFile f(info.absoluteFilePath());
			if (f.open(QIODevice::ReadOnly)) {
				QCryptographicHash hasher(QCryptographicHash::Sha256);
				if (hasher.addData(&f)) {
					const auto hash = hasher.result();
					auto it = seenHashes.find(hash);
					if (it != seenHashes.end()) {
						// Duplicate found!
						stats.duplicatesFound++;
						stats.bytesSaved += sz;
						f.close();
						// Remove duplicate copy to reclaim space
						QFile::remove(info.absoluteFilePath());
						continue;
					} else {
						seenHashes.emplace(hash, info.absoluteFilePath());
					}
				}
			}
		}

		pruneCandidates.push_back(info);
	}

	// Prune old cold files if still over limit
	int64 currentSize = stats.totalBytes - stats.bytesSaved;
	if (targetLimitBytes > 0 && currentSize > targetLimitBytes) {
		for (const auto &info : pruneCandidates) {
			if (currentSize <= targetLimitBytes) {
				break;
			}
			const auto sz = info.size();
			if (QFile::remove(info.absoluteFilePath())) {
				currentSize -= sz;
				stats.bytesSaved += sz;
			}
		}
	}

	return stats;
}

} // namespace Storage
