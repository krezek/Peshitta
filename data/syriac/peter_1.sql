DROP TABLE IF EXISTS peter_1_sy;
CREATE VIRTUAL TABLE peter_1_sy USING FTS5(chapter, verse, prefix, suffix, body);

.read peter_1/peter_1_1.sql
.read peter_1/peter_1_2.sql
.read peter_1/peter_1_3.sql
.read peter_1/peter_1_4.sql
.read peter_1/peter_1_5.sql
