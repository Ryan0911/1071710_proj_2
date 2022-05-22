# 1071710_proj_1
## 此次project包含內容如下
- [x] 程式碼 - 高分版：3-bit_history_predictor_v2.cpp || 低分版：(low)3-bit_history_predictor.cpp 
- [x] 輸入輸出範例數組 - 低分版input: input.txt || 高分版input: input2.txt, input3.txt, input4.txt || 輸出範例：1071710_proj_2輸入輸出範例.docx
- [x] 說明文件 - README.md
- [ ] youtube影片網址(尚未更新)
### 其餘內容
* (high)3-bit_history_predictor.cpp為有bug之程式碼，可置之不理沒關係
---
## 程式流程說明
1. 為了方便使用者重複使用本程式，一開始將要求輸入input程式碼的檔名，若檔案不存在會要求使用者重新輸入
2. 輸入Entries數，若輸入0以下(含)的數字，會要求使用者重新輸入
3. 進入程式的核心流程，進行預測並將Address、PC目前指向的Instruction、每個Entry的predictor的狀態與過往Miss次數、目前Entry所使用的predictor以及其預測結果與實際結果的比較、並將該entry目前miss的次數皆印出來顯示(也就是預測失誤會+1，成功則與歷史狀態相符)
4. 根據PC指向的實際內容進行該Entry之Predictor的更新
5. 重複步驟 3 - 4 直到程式結束
6. 印出所有Register的內容供使用者參考
7. 輸入0，離開程式；若輸入其他數字(0除外)，則回到步驟1 
---
## 程式碼說明
---
