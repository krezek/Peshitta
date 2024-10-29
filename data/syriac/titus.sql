DROP TABLE IF EXISTS titus_sy;
CREATE VIRTUAL TABLE titus_sy USING FTS5(chapter, verse, prefix, suffix, body);

.read titus/titus_1.sql
.read titus/titus_2.sql
.read titus/titus_3.sql
