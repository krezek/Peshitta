DROP TABLE IF EXISTS ephesians_sy;
CREATE VIRTUAL TABLE ephesians_sy USING FTS5(chapter, verse, prefix, suffix, body);

.read ephesians/ephesians_1.sql
.read ephesians/ephesians_2.sql
.read ephesians/ephesians_3.sql
.read ephesians/ephesians_4.sql
.read ephesians/ephesians_5.sql
.read ephesians/ephesians_6.sql
