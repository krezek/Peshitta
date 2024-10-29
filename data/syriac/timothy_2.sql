DROP TABLE IF EXISTS timothy_2_sy;
CREATE VIRTUAL TABLE timothy_2_sy USING FTS5(chapter, verse, prefix, suffix, body);

.read timothy_2/timothy_2_1.sql
.read timothy_2/timothy_2_2.sql
.read timothy_2/timothy_2_3.sql
.read timothy_2/timothy_2_4.sql
