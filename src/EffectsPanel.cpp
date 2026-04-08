#include "EffectsPanel.h"
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

namespace cinemastudio {

EffectsPanel::EffectsPanel(QWidget *parent) : QWidget(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(8, 8, 8, 8);
  layout->setSpacing(8);

  auto *search = new QLineEdit(this);
  search->setPlaceholderText("🔍 Search effects...");
  layout->addWidget(search);

  auto *list = new QListWidget(this);
  list->setViewMode(QListView::IconMode);
  list->setGridSize(QSize(80, 80));
  list->setSpacing(6);

  struct Effect {
    QString icon, name;
  };
  QVector<Effect> effects = {
      {"🌫️", "Blur"},      {"✨", "Sharpen"}, {"🎨", "Color"},
      {"☀️", "Brightness"}, {"🌈", "Hue/Sat"}, {"🟢", "ChromaKey"},
      {"⭕", "Vignette"},  {"🟤", "Sepia"},   {"⚫", "Grayscale"},
      {"🔄", "Invert"},    {"📽️", "Fade In"}, {"📽️", "Fade Out"},
      {"🔀", "Crossfade"}, {"➡️", "Wipe"},     {"↕️", "Slide"},
      {"🔍", "Zoom"}};

  for (const auto &fx : effects) {
    auto *item = new QListWidgetItem(fx.icon + "\n" + fx.name);
    item->setTextAlignment(Qt::AlignCenter);
    list->addItem(item);
  }

  layout->addWidget(list, 1);
  layout->addWidget(new QLabel("16 effects available", this));
}

} // namespace cinemastudio
