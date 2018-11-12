#ifndef DRAWCURSOR_H
#define DRAWCURSOR_H

#include <QObject>
#include <QPoint>
#include <QLabel>
#include <QPixmap>
#include <QSize>

class DrawCursor : public QObject
{
    Q_OBJECT
public:
//    explicit DrawCursor(QObject *parent = nullptr);
    explicit DrawCursor(QWidget *widget, QObject *parent = nullptr);

signals:

public slots:

private:
    QLabel *cursorLabel;
    QWidget *showWidget;

private:
    QPixmap cursorPixmap;

public:
    void drawCursor(QSize sourceScreenSize, QPoint masterCursorPos);
};

#endif // DRAWCURSOR_H
