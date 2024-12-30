#include "TeethPreview.h"
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QVBoxLayout>
#include <algorithm>
//#include "Messagebox.h"
#include"QMessageBox"
TeethPreview::TeethPreview(QWidget* parent)

     :m_image_start_x(0)
    , m_image_start_y(0)
{   this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    resize(800, 600);  // 设置窗口的初始大小为 800x600
    updateUpperOrLower(true);  // 初始化为上牙视图
    m_titleBar = new MyTitleBar(this);
    m_titleBar->move(0, 0);
    m_titleBar->setBackgroundColor(15,15,15);
    m_titleBar->setTitleContent(tr("Select Teeth"));
    m_titleBar->setTitleIcon(":/icon/icon.png");

    connect(m_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
    connect(m_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
    connect(m_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
    connect(m_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));
    m_finish_button = new QPushButton(tr("Done"), this);



    {
        m_finish_button->setFixedSize(180,40);
        m_finish_button->setStyleSheet(R"(
        QPushButton {
            background-color: #5D5D5D; /* 默认状态下背景颜色为 #5D5D5D */
            color: white;               /* 默认字体颜色为白色 */
            border-radius: 8px;
            padding: 8px;
            font: 18px 'Microsoft Yahei';
        }
        QPushButton:hover {
            background: #1296db; /* 悬停时渐变背景 */
            color: white;               /* 悬停时字体颜色为白色 */
        }
        QPushButton:pressed {
            background: #1296db; /* 点击时渐变背景 */
            color: white;               /* 点击时字体颜色为白色 */
        }
    )");
    }


    m_layout = new QVBoxLayout(this);



    m_layout->addStretch();
    m_layout->addWidget(m_finish_button, 0, Qt::AlignCenter);
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->setContentsMargins(0,40,0,0);
    setLayout(m_layout);

    connect(m_finish_button, &QPushButton::clicked, this, [=]() {
        for(auto i : m_selected_teeth){
            qDebug()<<i;
        }
        qDebug() << "Selection finished.";
        emit choseNumsOfTeeth(m_selected_teeth.size());
        if(m_selected_teeth.size()==0){
            QMessageBox m_box(QMessageBox::Information, tr("Prompt Information"), tr("Please select at least one!"),QMessageBox::Ok);
            qDebug()<<"unchose any tooth";
        }else{

            this->close();

        }

    });

}

void TeethPreview::updateUpperOrLower(bool isUpper)
{
    m_current_teeth_id = -1;
    clearAll();

    if (!isUpper) {
        m_mask_image.load("./resources/pic/lower_teeth_mask.bmp");
        m_origin_image.load("./resources/pic/lower_teeth.png");
    } else {
        m_mask_image.load("./resources/pic/upper_teeth_mask.bmp");
        m_origin_image.load("./resources/pic/upper_teeth.png");
    }

    m_mask_image = m_mask_image.convertToFormat(QImage::Format_Grayscale8);
    m_mask_width = m_mask_image.width();
    m_mask_height = m_mask_image.height();

    m_backup_image = m_origin_image;
    m_image_width = m_origin_image.width();
    m_image_height = m_origin_image.height();

    initTeethName();
    initTeethPos();
    update();
}

void TeethPreview::clearAll()
{
    m_teeth_name_map.clear();
    m_teeth_pos_map.clear();
    for (int i = 1; i <= 32; i++) {
        m_teeth_checked[i] = false;
    }
    m_selected_teeth.clear();
}

void TeethPreview::initTeethName()
{
    // 初始化牙齿名称映射
    m_teeth_name_map[1] = 17;
    m_teeth_name_map[2] = 18;
    m_teeth_name_map[3] = 48;
    m_teeth_name_map[4] = 47;
    m_teeth_name_map[5] = 16;
    m_teeth_name_map[6] = 46;
    m_teeth_name_map[7] = 15;
    m_teeth_name_map[8] = 45;
    m_teeth_name_map[9] = 14;
    m_teeth_name_map[10] = 44;
    m_teeth_name_map[11] = 13;
    m_teeth_name_map[12] = 43;
    m_teeth_name_map[13] = 12;
    m_teeth_name_map[14] = 42;
    m_teeth_name_map[15] = 11;
    m_teeth_name_map[16] = 41;
    m_teeth_name_map[17] = 21;
    m_teeth_name_map[18] = 31;
    m_teeth_name_map[19] = 32;
    m_teeth_name_map[20] = 22;
    m_teeth_name_map[21] = 33;
    m_teeth_name_map[22] = 23;
    m_teeth_name_map[23] = 34;
    m_teeth_name_map[24] = 24;
    m_teeth_name_map[25] = 35;
    m_teeth_name_map[26] = 25;
    m_teeth_name_map[27] = 36;
    m_teeth_name_map[28] = 26;
    m_teeth_name_map[29] = 37;
    m_teeth_name_map[30] = 27;
    m_teeth_name_map[31] = 28;
    m_teeth_name_map[32] = 38;
}

void TeethPreview::initTeethPos()
{
    unsigned char *data = m_mask_image.bits();
    int num[33] = {0};
    double px[33] = {0};
    double py[33] = {0};

    for (int i = 0; i < m_mask_height; i++) {
        for (int j = 0; j < m_mask_width; j++) {
            int xx = data[i * m_mask_width + j];
            num[xx]++;
            px[xx] += j;
            py[xx] += i;
        }
    }
    for (int i = 1; i < 33; i++) {
        px[i] /= num[i];
        py[i] /= num[i];
        m_teeth_pos_map[i] = QPoint(px[i], py[i]);
    }
}

int TeethPreview::imageClickd(int x, int y)
{
    int indX = m_mask_width * x / m_image_width;
    int indY = m_mask_height * y / m_image_height;
    unsigned char* mask = m_mask_image.bits();
    int mv = mask[indY * m_mask_width + indX];
    if (mv <= 0 || mv > 32)
        return -1;
    return mv;
}

void TeethPreview::updateToothImage()
{
    unsigned char *data = m_origin_image.bits();
    unsigned char* mask = m_mask_image.bits();
    unsigned char *bak_data = m_backup_image.bits();

    for (int i = 0; i < m_image_height; i++) {
        for (int j = 0; j < m_image_width; j++) {
            // 恢复原始图像数据
            data[i * m_image_width * 4 + j * 4] = bak_data[i * m_image_width * 4 + j * 4];
            data[i * m_image_width * 4 + j * 4 + 1] = bak_data[i * m_image_width * 4 + j * 4 + 1];
            data[i * m_image_width * 4 + j * 4 + 2] = bak_data[i * m_image_width * 4 + j * 4 + 2];

            // 获取当前像素对应的牙齿编号
            int maskX = m_mask_width * j / m_image_width;
            int maskY = m_mask_height * i / m_image_height;
            int v = mask[maskY * m_mask_width + maskX];

            // 改变已选中牙齿的颜色
            if (m_teeth_checked[v]) {
                data[i * m_image_width * 4 + j * 4] = 0;
                data[i * m_image_width * 4 + j * 4 + 1] = 200;
                data[i * m_image_width * 4 + j * 4 + 2] = 0;
            }
        }
    }
}

void TeethPreview::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(44,44,44)); // 画一个 #2C2C2C 背景
    QPixmap pixmap;
    pixmap.convertFromImage(m_origin_image);
    pixmap = pixmap.scaled(m_image_width, m_image_height, Qt::KeepAspectRatio);

    // 计算图像的起始位置，使其居中显示
    m_image_start_x = (width() - m_image_width) / 2;
    m_image_start_y = (height() - m_image_height) / 2;

    painter.drawPixmap(m_image_start_x, m_image_start_y, pixmap);

    painter.setPen(QPen(QColor(255, 255, 255), 2));
    QFont font = this->font();
    font.setPixelSize(22);
    painter.setFont(font);

    for (int i = 1; i <= 32; i++) {

        if (!m_teeth_checked[i])
            continue;
        int mid = m_teeth_name_map[i];
        QPoint p = QPoint(m_teeth_pos_map[i].x() * m_image_width / m_mask_width + m_image_start_x,
                          m_teeth_pos_map[i].y() * m_image_height / m_mask_height + m_image_start_y);
        QPoint p2, p3;

        // 计算标签绘制位置
        if ((mid >= 11 && mid <= 18) || (mid >= 41 && mid <= 48)) {
            p2 = QPoint(p.x() - 60, p.y());
            p3 = QPoint(p2.x() - 77, p2.y() + 7);
        } else {
            p2 = QPoint(p.x() + 60, p.y());
            p3 = QPoint(p2.x() + 2, p2.y() + 7);
        }

        painter.drawLine(p, p2);
        painter.drawText(p3, QString::number(m_teeth_name_map[i]));
    }
}

void TeethPreview::mousePressEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();

    if (event->button() == Qt::LeftButton) {
        int id = imageClickd(x - m_image_start_x, y - m_image_start_y);
        m_current_teeth_id = id;

        if (id != -1) {
            m_teeth_checked[id] = !m_teeth_checked[id];  // 切换选中状态
            if (m_teeth_checked[id]) {
                m_selected_teeth.push_back(m_teeth_name_map[id]);  // 添加到选中数组
            } else {
                m_selected_teeth.erase(std::remove(m_selected_teeth.begin(), m_selected_teeth.end(), m_teeth_name_map[id]), m_selected_teeth.end());  // 从数组中移除
            }
            qDebug() << (m_teeth_checked[id] ? "Selected tooth ID:" : "Unselected tooth ID:") << m_teeth_name_map[id];
            updateToothImage();  // 更新牙齿图像
            update();  // 触发重绘
        }
    }

    QDialog::mousePressEvent(event);  // 调用父类的 mousePressEvent 处理其他事件
}
void TeethPreview::onButtonMinClicked()
{
    showMinimized();  // Minimize the window
}

void TeethPreview::onButtonRestoreClicked()
{
    // Handle window restore logic (maximize -> restore)
    if (isMaximized()) {
        showNormal();  // Restore the window to its normal state
    } else {
        showMaximized();  // Maximize the window
    }
}

void TeethPreview::onButtonMaxClicked()
{
    showMaximized();  // Maximize the window
}

void TeethPreview::onButtonCloseClicked()
{
    close();  // Close the window
}
