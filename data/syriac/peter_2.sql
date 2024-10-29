DROP TABLE IF EXISTS peter_2_sy;
CREATE VIRTUAL TABLE peter_2_sy USING FTS5(chapter, verse, prefix, suffix, body);

.read peter_2/peter_2_1.sql
.read peter_2/peter_2_2.sql
.read peter_2/peter_2_3.sql
