# 🌿 EcoGram – High-Performance & Anti-Bloat Telegram Desktop Fork

**EcoGram** — это форк официального клиента Telegram Desktop (Windows x64), созданный с фокусом на экстремальную производительность, освобождение дискового пространства и чистоту оперативной памяти.

[![GitHub Actions Build](https://img.shields.io/badge/Build-GitHub%20Actions%20(Windows%20x64)-blue?logo=github-actions)](https://github.com/nxvr443/EcoGram/actions)
[![License](https://img.shields.io/badge/License-GPLv3%20with%20OpenSSL-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20x64-0078D6?logo=windows)](https://github.com/nxvr443/EcoGram)

---

## 🚀 Ключевые возможности

### 1. 🛡️ SmartCache Engine (Борьба с раздуванием `tdata`)
В оригинальном клиенте Telegram папка кэша `tdata` разрастается до десятков гигабайт. **EcoGram** решает эту проблему на уровне C++ ядра:
- **Контентная дедупликация (Content-Defined Deduplication):** если один и тот же мем, стикер или аватарка пересланы в сотни чатов, они сохраняются на диск **только один раз**.
- **Фоновое сжатие и уплотнение:** метаданные, карты и стикеры автоматически пакуются без нагрузки на процессор.
- **Квотирование «мусорных» каналов:** автоматическая фоновая очистка холодного кэша каналов, которые вы давно не открывали.
- **Нулевые задержки (Zero Latency):** доступ к медиа в миллисекундах через асинхронный VFS-слой.

### 2. 🧊 План развития (Roadmap)
- [x] **SmartCache & Anti-Bloat:** дедупликация и предотвращение разрастания `tdata` до 3+ ГБ мусора.
- [x] **Windows x64 Cloud CI:** чистый автоматический билд через GitHub Actions без зависимостей от локальных путей.
- [ ] **RAM Freezer:** интеллектуальная выгрузка неактивных текстур и чатов при сворачивании в трей (падение аппетита до ~100 МБ RAM).
- [ ] **Multi-Stream Turbo Downloader:** параллельное скачивание файлов через несколько MTProto-сессий (до 3–5x быстрее).
- [ ] **Clean UI / Детокс:** полное выпиливание сторис, спонсорских постов и лишних баннеров.
- [ ] **Ghost Mode & Audit:** скрытный просмотр чатов и локальное сохранение удаленных/отредактированных сообщений.

---

## 🛠️ Сборка проекта (Build)

Проект оптимизирован для сборки через облако **GitHub Actions**:

1. Откройте вкладку **[Actions](https://github.com/nxvr443/EcoGram/actions)**.
2. Выберите воркфлоу **Build Fork (Windows x64)**.
3. Нажмите **Run workflow** (выбрав ветку `main` и конфигурацию `Release`).
4. По окончании сборки скачайте архив `TelegramFork-Windows-x64.zip` с готовым исполняемым файлом `Telegram.exe` из раздела **Artifacts**.

---

## 📜 Лицензия

EcoGram распространяется под лицензией **GNU General Public License v3** с исключением OpenSSL (на базе официального кода Telegram Desktop).
