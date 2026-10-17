/*
This file is part of Telegram Desktop fork.
Smart Cache Manager: Anti-Bloat, Deduplication, and Background Compaction Engine.
*/
#pragma once

#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <crl/crl_time.h>

namespace Storage {

class SmartCache final {
public:
	struct Stats {
		int64 totalFiles = 0;
		int64 totalBytes = 0;
		int64 duplicatesFound = 0;
		int64 bytesSaved = 0;
	};

	static SmartCache &Instance();

	// Starts background maintenance on the cache directory
	void startBackgroundMaintenance(const QString &userBasePath);

	// Runs a single pass of deduplication and size clamping
	Stats runOptimizationPass(const QString &cachePath, int64 targetLimitBytes);

	// Configurable limits
	void setMaxChannelCacheSize(int64 bytes) { _maxChannelCacheSize = bytes; }
	[[nodiscard]] int64 maxChannelCacheSize() const { return _maxChannelCacheSize; }

private:
	SmartCache() = default;
	~SmartCache() = default;

	SmartCache(const SmartCache &) = delete;
	SmartCache &operator=(const SmartCache &) = delete;

	int64 _maxChannelCacheSize = 1024 * 1024 * 1024; // 1 GB default cap
	bool _maintenanceRunning = false;
};

} // namespace Storage
