import sqlite3

db_names = ['0.db','1.db','2.db','3.db']

all_data = []

for i in db_names:
    conn = sqlite3.connect(i)
    c = conn.cursor()
    for j in c.execute("SELECT tile_2048, tile_1024, moves, param1, param2, param3, param4 FROM scores"):
        tile_2048 = j[0]
        tile_1024 = j[1]
        moves = j[2]
        param1 = j[3]
        param2 = j[4]
        param3 = j[5]
        param4 = j[6]
        all_data.append([float(tile_2048),float(tile_1024),float(moves),float(param1),float(param2),float(param3),float(param4)])
    conn.close()

all_data.sort(key=lambda x:x[2],reverse=True)

for i in all_data[:10]:
    print i
