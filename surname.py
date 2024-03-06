# Read output
with open("output_05_03_2024.txt", "r") as f:
    piani = f.read()
SPLIT = "\n\n\n\n**********************************************************"
listapiani = piani.split(SPLIT)
print(len(listapiani))
listapiani = ["\n\n\n"+listapiani[0]]+[SPLIT+p for p in listapiani[1:]]

names = [p.split("\n")[5].split(" ")[0] for p in listapiani]
surnames = [p.split("\n")[5].split(" ")[1] for p in listapiani]
listapianisur = [piano for _,piano in sorted(zip(surnames, listapiani))]
print(len(listapianisur))
input()
listapianisur = "".join(listapianisur)
print(listapianisur)

# Save output
with open("output_05_03_2024_surname.txt", "w") as f:
    f.write(listapianisur)