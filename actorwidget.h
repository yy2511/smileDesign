#ifndef ACTORWIDGET_H
#define ACTORWIDGET_H

#include <QObject>
#include <QWidget>
#include "qlabel.h"
#include "QHBoxLayout"
#include <QVBoxLayout>

#include "QPushButton"
#include "QListWidget"
#include "QSlider"
#include "vtkActor.h"
#include "vtkProperty.h"
#include <QDebug>
#include "vtkSmartPointer.h"
#include<iostream>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QFont>
#include <QFontDatabase>
class CustomComboBoxDelegate : public QStyledItemDelegate {
public:
    CustomComboBoxDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // 设置图标的大小为一致的尺寸
        opt.decorationSize = QSize(100, 50); // 将图标大小设置为 200x200

        QStyledItemDelegate::paint(painter, opt, index);
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        return QSize(size.width(), 60); // 调整选项项的高度以匹配图标的大小
    }
};
class UserItemWidget : public QWidget
{
    Q_OBJECT
public:
    UserItemWidget(const QString &name, std::vector<vtkSmartPointer<vtkActor>> actors,QWidget *parent =nullptr):QWidget(parent){
        QFont font;
        font.setFamily("Microsoft Yahei");   // 设置字体类型，例如 "Arial"




        QVBoxLayout *mainLayout = new QVBoxLayout(this); // 使用垂直布局将标签和滑块垂直排列
        setLayout(mainLayout);
        this->actors = actors;


        label = new QLabel(this);
        label->setText(name);

        label->setStyleSheet("color: white;"); // 设置字体颜色为 #2C2C2C

        // 使用 findChildren 查找并设置标签字体
        QList<QLabel*> labels = this->findChildren<QLabel*>();
        for (QLabel *lbl : labels) {
            lbl->setFont(font);
        }
        QSlider *slider = new QSlider(this);
        slider->setOrientation(Qt::Horizontal);
        slider->setRange(0,100);
        slider->setValue(100);
        QString sliderStyle =
            "QSlider::groove:horizontal {"
            "background: #d3d3d3;"                   // 滑轨背景颜色
            "height: 10px;"
            "border-radius: 5px;"
            "}"

            "QSlider::handle:horizontal {"
            "background: #73C4F6;" // 滑块渐变颜色
            "width: 10px;"                          // 滑块宽度
            "height: 10px;"                         // 滑块高度
            "margin: -5px 0;"                       // 调整滑块的位置
            "border-radius: 10px;"                  // 滑块圆角
            "}"

            "QSlider::add-page:horizontal {"
            "background: #d3d3d3;"                  // 未滑动部分颜色
            "border-radius: 5px;"
            "}"

            "QSlider::sub-page:horizontal {"
            "background: #1296db;" // 滑动部分渐变颜色
            "border-radius: 5px;"
            "}";

        slider->setStyleSheet(sliderStyle);
        // 添加标签和滑块到布局
        mainLayout->addWidget(label,0);
        mainLayout->addWidget(slider,1);
        mainLayout->setContentsMargins(0, 0, 0, 0);  // 减小边距
        mainLayout->setSpacing(3);  // 设置控件间距

        connect(slider,&QSlider::valueChanged,this,&UserItemWidget::onSliderValueChanged);
    }

    QString getData(){
        return label->text();
    }

private:
    std::vector<vtkSmartPointer<vtkActor>> actors;

    QLabel *label;

signals:
    void refresh();
private slots:
    void onSliderValueChanged(int value){
        if(actors.size()!=0)
        {
            for (auto a:actors) {
                double v = static_cast<double>(value)/100 ;
                a->GetProperty()->SetOpacity(v);
                qDebug() << "Actor opacity set to:" << a->GetProperty()->GetOpacity();
            }
        }

        emit refresh();
    }

};
class ActorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ActorWidget(QWidget *parent = nullptr);
    void adjustListWidgetHeight();
signals:
    void refresh();
private slots:
    void addItem(const QString& name,std::vector<vtkSmartPointer<vtkActor>> actors );
    void removeItem(const QString& name);
private:
    UserItemWidget *citem;
    QListWidget *w;
};

class TipWidget : public QWidget
{
public:
    //TipWidget(){}

    TipWidget(const QString &text, const QPixmap &pixmap, QWidget *parent = nullptr)
        : QWidget(parent)
    {
        // 去除窗口边框和标题栏，只保留内容显示
        setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
        // 设置布局
        layout = new QVBoxLayout(this);  // 保存布局指针

        // 创建并保存 QLabel，用于显示图片
        imageLabel = new QLabel(this);
        layout->addWidget(imageLabel);

        // 创建并保存 QLabel，用于显示文字
        textLabel = new QLabel(text, this);
        textLabel->setStyleSheet("font-size: 15px; color: white;"); // 设置字体大小为18px，颜色为白色
        layout->addWidget(textLabel);

        // 初始显示
        updateTip(text, pixmap);
    }

    // 添加用于更新内容的函数
    void updateTip(const QString &newText, const QPixmap &newPixmap)
    {   qDebug()<<"newText"<<newText<<endl;
        std::cout<< "The address of this textLabel is: " << textLabel << std::endl;
        if(!textLabel){
            cout<<"textLabel is null"<<std::endl;
        }

        textLabel->setText(newText);  // 更新文字
        std::cout<<11111<<std::endl;
        imageLabel->setPixmap(newPixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));  // 更新图片
        adjustSize();  // 根据新内容调整大小
    }

private:
    QVBoxLayout *layout;   // 布局指针
    QLabel *imageLabel;    // 显示图片的 QLabel
    QLabel *textLabel;     // 显示文字的 QLabel
};



#endif // ACTORWIDGET_H
