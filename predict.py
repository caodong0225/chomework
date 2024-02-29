import numpy as np
from keras.models import load_model
import pickle

model = load_model("test.h5")
with open('test.pickle', 'rb') as f:
    tokenizer_string = pickle.load(f)
model.summary()

print("请输入三个词语，用逗号分开")
data = input().split(",")
print("输入期待的标题长度:")
n = int(input())
while len(data) < n:
    input_sequence = tokenizer_string.texts_to_sequences([data[-3:]])
    # print(input_sequence)
    input_seq_length = 3
    X = np.reshape(input_sequence, (len(input_sequence), input_seq_length, 1))
    res = np.argmax(model.predict(X))
    # print(res)
    dict_index = tokenizer_string.word_index
    for i in dict_index.items():
        if i[1] == res:
            data.append(i[0])
            break
print("".join(data))
