#include "DatabaseManager.h"

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) m_db.close();
}


//데이터 베이스 초기화
bool DatabaseManager::initDatabase(const QString& dbName) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbName);

    if (!m_db.open()) {
        qDebug() << "DB 연결 실패:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query;
    QString createTable =
        "CREATE TABLE IF NOT EXISTS schedules ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "title TEXT NOT NULL, "
        "content TEXT, "
        "start_time TEXT NOT NULL, "
        "end_time TEXT NOT NULL, "
        "color TEXT"
        ")";

    return query.exec(createTable);
}

bool DatabaseManager::addSchedule(const QString& title, const QString& content,
                                  const QDateTime& start, const QDateTime& end, const QString& color) {
    QSqlQuery query;
    query.prepare("INSERT INTO schedules (title, content, start_time, end_time, color) "
                  "VALUES (:title, :content, :start, :end, :color)");

    // ISO 8601 형식(yyyy-MM-ddTHH:mm:ss)으로 저장하면 나중에 정렬하기 매우 쉽습니다.
    query.bindValue(":start", start.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":end", end.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":title", title);
    query.bindValue(":content", content);
    query.bindValue(":color", color);

    return query.exec();
}

QList<QVariantMap> DatabaseManager::getSchedulesForMonth(int year, int month) {
    QList<QVariantMap> schedules;
    QSqlQuery query;

    // 해당 월에 걸쳐있는 일정을 모두 가져오는 쿼리
    QString datePattern = QString("%1-%2").arg(year).arg(month, 2, 10, QChar('0'));
    query.prepare("SELECT * FROM schedules WHERE start_date LIKE :pattern OR end_date LIKE :pattern");
    query.bindValue(":pattern", datePattern + "%");

    if (query.exec()) {
        while (query.next()) {
            QVariantMap item;
            item["id"] = query.value("id");
            item["title"] = query.value("title");
            item["start_date"] = query.value("start_date");
            item["end_date"] = query.value("end_date");
            item["color"] = query.value("color");
            schedules.append(item);
        }
    }
    return schedules;

}

QList<QVariantMap> DatabaseManager::getSchedulesForDay(const QDate& date) {
    QList<QVariantMap> schedules;
    QSqlQuery query;

    QString dateStr = date.toString("yyyy-MM-dd");
    // 해당 날짜에 포함된 일정을 시작 시간 순으로 정렬하여 가져옴
    query.prepare("SELECT * FROM schedules WHERE start_time LIKE :date "
                  "ORDER BY start_time ASC");
    query.bindValue(":date", dateStr + "%");

    if (query.exec()) {
        while (query.next()) {
            QVariantMap item;
            item["title"] = query.value("title");
            item["start"] = query.value("start_time"); // "2026-04-20 14:30:00"
            schedules.append(item);
        }
    }
    return schedules;
}