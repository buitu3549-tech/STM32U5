import sys
sys.stdout.reconfigure(encoding='utf-8')
f = open(r'E:\项目\归星纪\归星纪-项目设计文档.md','r',encoding='utf-8')
lines = f.readlines()
f.close()
# Section 九: 伏笔
for i in range(270, min(330, len(lines))):
    print(f'{i}:{lines[i]}',end='')
