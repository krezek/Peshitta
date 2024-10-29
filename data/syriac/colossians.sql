DROP TABLE IF EXISTS colossians_sy;
CREATE VIRTUAL TABLE colossians_sy USING FTS5(chapter, verse, prefix, suffix, body);

.read colossians/colossians_1.sql
.read colossians/colossians_2.sql
.read colossians/colossians_3.sql
.read colossians/colossians_4.sql
