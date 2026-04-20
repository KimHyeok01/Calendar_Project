#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (DatabaseManager::instance().initDatabase("calendar_data.db")) {
        qDebug() << "---------------------------------------";
        qDebug() << "SUCCESS: Database initialized.";
    }

    // 2. 테스트용 샘플 데이터 생성
    // 현재 시간부터 3시간 동안 진행되는 일정 예시
    QDateTime startTime = QDateTime::currentDateTime();
    QDateTime endTime = startTime.addSecs(50 * 3600); // 3시간 후

    bool success = DatabaseManager::instance().addSchedule(
        "임베디드 프로젝트 테스트",
        "타임트리 스타일 DB 테스트 중",
        startTime,
        endTime,
        "#FF5733"
        );

    if (success) {
        qDebug() << "SUCCESS: Sample schedule added.";
    }

    // 3. 데이터 조회 테스트 (오늘 날짜 기준)
    qDebug() << "CHECK: Loading schedules for today...";
    QList<QVariantMap> todaySchedules = DatabaseManager::instance().getSchedulesForDay(startTime.date());

    for (const QVariantMap& schedule : todaySchedules) {
        qDebug() << "=======================================";
        qDebug() << "ID:      " << schedule["id"].toInt();
        qDebug() << "Title:   " << schedule["title"].toString();
        qDebug() << "Start:   " << schedule["start"].toString();
        qDebug() << "End:     " << schedule["end"].toString();
        qDebug() << "Color:   " << schedule["color"].toString();
        qDebug() << "=======================================";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
