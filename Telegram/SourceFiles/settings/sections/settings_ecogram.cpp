/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_ecogram.h"

#include "core/application.h"
#include "core/core_settings.h"
#include "core/version.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "settings/settings_common.h"
#include "ui/boxes/confirm_box.h"
#include "ui/painter.h"
#include "ui/rect.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_settings.h"
#include "styles/style_menu_icons.h"
#include "styles/style_boxes.h"

#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>

namespace Settings {

EcoGram::EcoGram(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

rpl::producer<QString> EcoGram::title() {
	return rpl::single(u"🌿 Настройки EcoGram"_q);
}

void EcoGram::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	Ui::AddSkip(content, st::settingsPrivacySkip);

	// --- Header Branding ---
	const auto header = content->add(
		object_ptr<Ui::VerticalLayout>(content));

	header->add(
		object_ptr<Ui::FlatLabel>(
			header,
			rpl::single(u"EcoGram Desktop v" + QString::fromLatin1(EcoGramVersionStr) + u" ("_q + currentVersionText() + u")"_q),
			st::settingsSubsectionTitle),
		st::boxRowPadding);

	const auto descLabel = header->add(
		object_ptr<Ui::FlatLabel>(
			header,
			rpl::single(u"Форк Telegram Desktop с расширенной кастомизацией, режимом призрака и сохранением удаленных сообщений."_q),
			st::boxLabel),
		st::boxRowPadding);
	descLabel->setTextColorOverride(st::windowSubTextFg->c);

	Ui::AddSkip(content, st::settingsPrivacySkip);
	Ui::AddDivider(content);

	// --- 1. Режим призрака (Ghost Mode) ---
	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Режим призрака (Ghost Mode)"_q));

	// Master Ghost Mode Toggle
	const auto masterGhostBtn = AddButtonWithIcon(
		content,
		rpl::single(u"Режим призрака (Основной)"_q),
		st::settingsButton,
		{ &st::menuIconStealth });
	masterGhostBtn->toggleOn(Core::App().settings().ghostModeValue());
	masterGhostBtn->toggledChanges(
	) | rpl::filter([=](bool toggled) {
		return (toggled != Core::App().settings().ghostMode());
	}) | rpl::on_next([=](bool toggled) {
		Core::App().settings().setGhostMode(toggled);
		Core::App().saveSettingsDelayed();
	}, masterGhostBtn->lifetime());

	// Detailed sub-options inside slide wrap (active when ghost mode is on)
	const auto ghostOptionsWrap = content->add(
		object_ptr<Ui::SlideWrap<Ui::VerticalLayout>>(
			content,
			object_ptr<Ui::VerticalLayout>(content)));
	const auto ghostOptions = ghostOptionsWrap->entity();

	// 1a. Don't mark messages as read
	const auto readHistoryCheck = ghostOptions->add(
		object_ptr<Ui::Checkbox>(
			ghostOptions,
			u"Не читать входящие сообщения"_q,
			Core::App().settings().ghostReadHistory(),
			st::settingsCheckbox),
		st::settingsCheckboxPadding);
	readHistoryCheck->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Core::App().settings().setGhostReadHistory(checked);
		Core::App().saveSettingsDelayed();
	}, readHistoryCheck->lifetime());

	// 1b. Don't mark stories as read
	const auto readStoriesCheck = ghostOptions->add(
		object_ptr<Ui::Checkbox>(
			ghostOptions,
			u"Не читать истории (Stealth Stories)"_q,
			Core::App().settings().ghostReadStories(),
			st::settingsCheckbox),
		st::settingsCheckboxPadding);
	readStoriesCheck->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Core::App().settings().setGhostReadStories(checked);
		Core::App().saveSettingsDelayed();
	}, readStoriesCheck->lifetime());

	// 1c. Don't send typing status
	const auto typingCheck = ghostOptions->add(
		object_ptr<Ui::Checkbox>(
			ghostOptions,
			u"Не отправлять статус набора текста (Typing...)"_q,
			Core::App().settings().ghostTyping(),
			st::settingsCheckbox),
		st::settingsCheckboxPadding);
	typingCheck->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Core::App().settings().setGhostTyping(checked);
		Core::App().saveSettingsDelayed();
	}, typingCheck->lifetime());

	ghostOptionsWrap->toggleOn(Core::App().settings().ghostModeValue());

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		rpl::single(u"При включенном режиме собеседники не увидят отметки о прочтении, просмотры историй и индикатор набора текста."_q));

	Ui::AddSkip(content);
	Ui::AddDivider(content);

	// --- 2. Сохранение сообщений и аудит (Anti-Delete) ---
	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Сохранение сообщений (Аудит)"_q));

	const auto antiDeleteBtn = AddButtonWithIcon(
		content,
		rpl::single(u"Сохранять удаленные сообщения"_q),
		st::settingsButton,
		{ &st::menuIconDelete });
	antiDeleteBtn->toggleOn(Core::App().settings().antiDeleteValue());
	antiDeleteBtn->toggledChanges(
	) | rpl::filter([=](bool toggled) {
		return (toggled != Core::App().settings().antiDelete());
	}) | rpl::on_next([=](bool toggled) {
		Core::App().settings().setAntiDelete(toggled);
		Core::App().saveSettingsDelayed();
	}, antiDeleteBtn->lifetime());

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		rpl::single(u"Сообщения, удаленные собеседником, не исчезают из вашего диалога и помечаются тегом [удалено]."_q));

	Ui::AddSkip(content);
	Ui::AddDivider(content);

	// --- 3. Ссылки и проект EcoGram ---
	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"О проекте EcoGram"_q));

	const auto repoBtn = AddButtonWithIcon(
		content,
		rpl::single(u"Репозиторий GitHub"_q),
		st::settingsButton,
		{ &st::menuIconLink });
	repoBtn->setClickedCallback([=] {
		QDesktopServices::openUrl(QUrl(u"https://github.com/nxvr443/EcoGram"_q));
	});

	const auto versionBtn = AddButtonWithLabel(
		content,
		rpl::single(u"Сведения о сборке"_q),
		rpl::single(u"v" + QString::fromLatin1(EcoGramVersionStr)),
		st::settingsButton,
		{ &st::menuIconInfo });
	versionBtn->setClickedCallback([=] {
		controller->show(Ui::MakeInformBox(
			u"EcoGram Desktop\n\n"
			u"Версия клиента: v" + QString::fromLatin1(EcoGramVersionStr) + u" (Сборка " + QString::number(EcoGramBuildNumber) + u")\n"
			u"Базовый стек: Telegram Desktop " + currentVersionText() + u"\n"
			u"Канал обновлений: Stable Release\n\n"
			u"Высокопроизводительный форк с оптимизацией сети, аппаратным ускорением, защитой от удаления и скрытным режимом."));
	});

	Ui::AddSkip(content, st::settingsPrivacySkip);

	Ui::ResizeFitChild(this, content);
}

Type EcoGramId() {
	return EcoGram::Id();
}

} // namespace Settings
