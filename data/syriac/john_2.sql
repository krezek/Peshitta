DROP TABLE IF EXISTS john_2_sy;
CREATE VIRTUAL TABLE john_2_sy USING FTS5(chapter, verse, prefix, suffix, body);

.read john_2/john_2_1.sql
