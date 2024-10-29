DROP TABLE IF EXISTS philemon_sy;
CREATE VIRTUAL TABLE philemon_sy USING FTS5(chapter, verse, prefix, suffix, body);

.read philemon/philemon_1.sql
