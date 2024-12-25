#include "ActorWidget.h"

ActorWidget::ActorWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();
    this->setLayout(layout);
    w = new QListWidget();
    layout->addWidget(w);
    layout->setContentsMargins(0,0,0,0);

    //this->setStyleSheet("border: none; ");
    w->setStyleSheet("border: none;");
    //w->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);  // 设置为只根据内容拉伸高度
    //w->setMaximumHeight(150);  // 或者明确设置最大高度
}
void ActorWidget::adjustListWidgetHeight()
{
    int totalHeight = 0;
    for (int i = 0; i < w->count(); ++i) {
        QListWidgetItem *item = w->item(i);
        totalHeight += w->sizeHintForRow(i);  // 获取每一项的高度
    }
    w->setFixedHeight(totalHeight);  // 根据所有项的总高度设置 QListWidget 的高度
}
void ActorWidget::addItem(const QString& name, std::vector<vtkSmartPointer<vtkActor>> actors) {
    UserItemWidget *itemW = new UserItemWidget(name, actors, this);
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(QSize(w->width(), 50));
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    w->addItem(item);
    w->setItemWidget(item, itemW);
    connect(itemW, &UserItemWidget::refresh, this, &ActorWidget::refresh);

    // 调整 QListWidget 高度
    adjustListWidgetHeight();
}

void ActorWidget::removeItem(const QString& name) {
    for (int i = 0; i < w->count(); ++i) {
        QListWidgetItem *item = w->item(i);
        UserItemWidget *itemW = dynamic_cast<UserItemWidget*>(w->itemWidget(item));
        if (name == itemW->getData() || name == "ALL") {
            w->removeItemWidget(item);
            delete w->takeItem(i);
            --i;  // 因为移除了项，调整索引
        }
    }
    // 调整 QListWidget 高度
    adjustListWidgetHeight();
}

