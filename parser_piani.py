# %%
import camelot
import os
from PyPDF2 import PdfReader

from PyPDF2 import PdfReader, PdfWriter

# Get name of PDF file
for file in os.listdir():
    if file.endswith(".pdf"):
        file_name = file
        break
print(f"> Found PDF file to process: {file_name}")
# Create folder named "split" if does not  exists
if not os.path.exists("split"):
    os.mkdir("split")
    print("> Created folder 'split'")
if not os.path.exists("output"):
    os.mkdir("output")
    print("> Created folder 'output'")

inputpdf = PdfReader(open(file_name, "rb"))
# If dir "split" is empty, split the pdf
if len(os.listdir("split")) == 0:
    print("> Splitting PDF file")
    for i in range(len(inputpdf.pages)):
        output = PdfWriter()
        output.add_page(inputpdf.pages[i])
        with open(os.path.join("split",f"page{i}.pdf"), "wb") as outputStream:
            output.write(outputStream)
else: 
    print("> Found split PDF files, skipping splitting")

# %%
folder = "split"
allnames = []
alreadyparsed = os.listdir("output")
fileerrors = []

text = []
print(f"> Found {len(os.listdir(folder))} files")
for nf,file in enumerate(os.listdir(folder)):
    print(f"> Processing file {nf+1}/{len(os.listdir(folder))}: ",end="")
    file = os.path.join(folder,file)
    table = camelot.read_pdf(file)
    try:
        table = table[0].df
    except:
        print(f"\n         >>  {file} is empty")
        fileerrors.append(file+" : Empty PDF, no name nor table\n\n")
        continue
    reader = PdfReader(file)
    page = reader.pages[0]  
    text = page.extract_text()
    header = text.split("AAACSemPosCodice Insegnamento CfuTipoTipo")[0]
    # media = text.split("Ripete")[-1]
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
<<<<<<< HEAD:parser.py
        fileerrors.append(file+" : No table found on this PDF\n\n")
=======
        # table = camelot.read_pdf(file+" : No name found on this PDF  \n\n")
        fileerrors.append(file+" : No name found on this PDF but a table has been found\n\n")
        # table = table[0].pdf
>>>>>>> piani marzo 2024:parser_piani.py
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

    media = """Media voti: (media riferita al piano approvato dell'anno precedente) 26.54 - Totale crediti piano: 130.0 - Totale crediti utili per la Laurea: 120.0 - Totale crediti insoprannumero: 10.0 - Totale crediti richiesti: 18.0"""
    
    final = header + "\n" + tablestring + media
    with open(os.path.join("output",name+".txt"), "w") as f:
        f.write(final)
        
with open("errors.txt", "w") as f:
    # Join all elements in list
    fileerrors = "".join(fileerrors)
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
    
# Read "PIANI_PARSED.txt" to a string
with open("PIANI_PARSED.txt", "r") as f:
    text = f.read()
text.replace