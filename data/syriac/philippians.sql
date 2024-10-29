DROP TABLE IF EXISTS philippians_sy;
CREATE VIRTUAL TABLE philippians_sy USING FTS5(chapter, verse, prefix, suffix, body);

.read philippians/philippians_1.sql
.read philippians/philippians_2.sql
.read philippians/philippians_3.sql
.read philippians/philippians_4.sql
