DROP TABLE IF EXISTS james_sy;
CREATE VIRTUAL TABLE james_sy USING FTS5(chapter, verse, prefix, suffix, body);

.read james/james_1.sql
.read james/james_2.sql
.read james/james_3.sql
.read james/james_4.sql
.read james/james_5.sql
