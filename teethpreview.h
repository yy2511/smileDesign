#ifndef TEETHPREVIEW_H
#define TEETHPREVIEW_H

#include <QFrame>
#include <QMap>
#include <QImage>
#include <QPoint>
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include"mytitlebar.h"
#include<QDialog>
class TeethPreview : public QDialog
{
    Q_OBJECT

public:
    explicit TeethPreview(QWidget* parent = nullptr);  // 构造函数
    void updateUpperOrLower(bool isUpper);  // 更新上/下牙视图
    std::vector<int>m_selected_teeth;
protected:
    void paintEvent(QPaintEvent *event) override;  // 用于绘制的事件处理函数
    void mousePressEvent(QMouseEvent *event) override;  // 处理鼠标点击事件

private:
    void clearAll();  // 清除牙齿状态
    void initTeethName();  // 初始化牙齿名称
    void initTeethPos();  // 初始化牙齿位置
    int imageClickd(int x, int y);  // 处理图片点击事件
    void updateToothImage();  // 更新牙齿图片


public slots:


    // Add these slots
    void onButtonMinClicked();
    void onButtonRestoreClicked();
    void onButtonMaxClicked();
    void onButtonCloseClicked();

private:
    QImage m_origin_image;  // 原始牙齿图像
    QImage m_mask_image;  // 牙齿的遮罩图像
    QImage m_backup_image;  // 备份的原始图像

    int m_image_start_x;  // 图像开始位置的x坐标
    int m_image_start_y;  // 图像开始位置的y坐标
    int m_image_width;  // 图像宽度
    int m_image_height;  // 图像高度
    int m_mask_width;  // 遮罩图像的宽度
    int m_mask_height;  // 遮罩图像的高度

    int m_current_teeth_id;  // 当前选择的牙齿ID
    QMap<int, bool> m_teeth_checked;  // 是否选择了某颗牙齿
    QMap<int, int> m_teeth_name_map;  // 牙齿名称映射
    QMap<int, QPoint> m_teeth_pos_map;  // 牙齿位置映射

    QPushButton* m_finish_button;  // 选择完成按钮

    QVBoxLayout* m_layout;  // 布局布局

    MyTitleBar* m_titleBar;

};

#endif // TEETHPREVIEW_H
