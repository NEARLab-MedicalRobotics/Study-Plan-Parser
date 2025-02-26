# Instructions for Processing the PDF  

## Step 1: Parse the PDF  
Run the **`parser_piani.py`** script on the original PDF.  
**Note:** The PDF should be the only file in the designated folder.  

**Output:**  
- `PIANI_PARSED.txt` – Contains all students with their chosen exams.  
- `errors.txt` – Lists pages from the original PDF that require manual verification.  

## Step 2: Verify Student Count  
Execute **`surname.py`** to check the number of students considered.  

## Step 3: Run the Piani Checker  
Once `PIANI_PARSED.txt` is corrected, compile and execute the Piani Checker with the following commands:  

```bash
g++ executable/PianiChecker.cpp
mv a.out PianiChecker3
./PianiChecker3 PIANI_PARSED.txt -E >> output.txt
```

**NOTE:** output.txt – The final processed text file.