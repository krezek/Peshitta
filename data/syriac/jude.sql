DROP TABLE IF EXISTS jude_sy;
CREATE VIRTUAL TABLE jude_sy USING FTS5(chapter, verse, prefix, suffix, body);

.read jude/jude_1.sql
