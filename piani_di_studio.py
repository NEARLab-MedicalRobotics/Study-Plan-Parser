# %%
import camelot
import os
from PyPDF2 import PdfReader
import pandas as pd

from PyPDF2 import PdfReader, PdfWriter

# Create folder named "split" if does not  exists
if not os.path.exists("split"):
    os.mkdir("split")
if not os.path.exists("output"):
    os.mkdir("output")

file_name = 'TuttiIPiani.pdf'
# pages = (121, 130)
from PyPDF2 import PdfWriter, PdfReader

inputpdf = PdfReader(open(file_name, "rb"))

for i in range(len(inputpdf.pages)):
    output = PdfWriter()
    output.add_page(inputpdf.pages[i])
    with open(os.path.join("split",f"page{i}.pdf"), "wb") as outputStream:
        output.write(outputStream)

# %%
folder = "split"
allnames = []
alreadyparsed = os.listdir("output")
fileerrors = []
print(f"> Found {len(os.listdir(folder))} files")
for nf,file in enumerate(os.listdir(folder)):
    print(f"> Processing file {nf+1}/{len(os.listdir(folder))}: ",end="")
    file = os.path.join(folder,file)
    table = camelot.read_pdf(file)
    try:
        table = table[0].df
    except:
        print(f"\n         >> Error on {file}")
        fileerrors.append(file)
        continue
    reader = PdfReader(file)
    page = reader.pages[0]  
    text = page.extract_text()
    header = text.split("AA AC Sem Pos Codice Insegnamento Cfu Tipo Tipo")[0]
    media = text.split("Ripete")[-1]
    header.replace(": ", ": \t")
    header = header.split("\n ")[-1]
    header.replace("Nome:", "\n\nNome:")
    try:
        name = header.split("Nome: ")[1].split("Matricola:")[0].strip().replace("\nCognome: ", "_").replace(" ", "")
        print(name, end="")
        if name+".txt" in alreadyparsed:
            print("--> ALREADY PARSED")
            continue 
        else:
            print()
    except:
        print(f"\n          >> Cannot find name on {file}")
        fileerrors.append(file)
        continue
    
    if name in allnames:
        print("DUPLICATE")
    allnames.append(name)
    
    matricola = header.split("Matricola: ")[1].split("Data")[0].strip()

    t = table[[0,1,2,3,6,4,5,12,13,14,15,16,17]].copy()
    t[5] = [i.replace("\n"," ") for i in t[5]]
    # t[5] = [i.replace('"', "") for i in t[5]]
    for idx,cfu in enumerate(t[6]):
        if len(cfu) == 1:
            cfu = "0" + cfu
            t[6][idx] = cfu
    # t[6] = [f" {int(cfu) : 02d} " for cfu in t[6]]
    tablestring = t.to_csv(sep = " ", index = False, header = False, path_or_buf=None)
    tablestring= tablestring.replace('"', "")

    final = header + "\n" + tablestring + media
    with open(os.path.join("output",name+".txt"), "w") as f:
        f.write(final)
        
with open("errors.txt", "w") as f:
    f.write(str(fileerrors))

# %%
folder = "output"
print(f"> Found {len(os.listdir(folder))} files")

output_file_bag = ""
for nf,file in enumerate(os.listdir(folder)):
    #Read file
    with open(os.path.join(folder,file), "r") as f:
        text = f.read()
        output_file_bag += text + "\n\n"
        
with open("PIANI_PARSED.txt", "w") as f:
    f.write(output_file_bag)