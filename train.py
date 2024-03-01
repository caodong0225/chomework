import jieba
from keras.preprocessing.text import Tokenizer
from keras.models import Sequential, load_model
from keras.layers import Dense, Embedding, LSTM, Dropout, Bidirectional
import pickle
import matplotlib.pyplot as plt
import numpy as np

data = []
data_all = []
dataset_in = []
dataset_out = []

# 读取训练数据
with open("text.txt", encoding="utf-8") as fr:
    for i in fr.readlines()[:4000]:  # 读取一部分数据训练（不要全读，带不动！）
        data.append(list(jieba.cut(i.strip("\n"))))

# 加载分词器
tokenizer_str = Tokenizer()
tokenizer_str.fit_on_texts(data)

length = 3  # 预测的词数，即：通过前length个词预测第四个词
# 生成训练数据
for sentence in data:
    for i in range(len(sentence) - length):
        if len(tokenizer_str.texts_to_sequences([sentence[i]])[0]):
            dataset_in.append(tokenizer_str.texts_to_sequences([sentence[i:i + length]]))
            dataset_out.append(tokenizer_str.texts_to_matrix([sentence[i + length]]))

dataset_out = np.array(dataset_out).squeeze()

num_char = len(tokenizer_str.word_index) + 1
print(num_char)  # 词典大小
dataset_in = np.array(dataset_in)
dataset_in = np.reshape(dataset_in, (len(dataset_in), length, 1))
print(dataset_in.shape)

# 构建模型
model = Sequential()
model.add(Embedding(num_char, 256, input_length=length))
model.add(LSTM(256, input_shape=(dataset_in.shape[1], dataset_in.shape[2]), return_sequences=True))
model.add(LSTM(256, return_sequences=True))
model.add(LSTM(256))
model.add(Dense(num_char, activation='softmax'))
model.summary()

model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
history = model.fit(dataset_in, dataset_out, batch_size=64, epochs=50, verbose=2)  # 训练模型
model.save("test.h5")
with open('test.pickle', 'wb') as handle:
    pickle.dump(tokenizer_str, handle, protocol=pickle.HIGHEST_PROTOCOL)
# 绘制训练 & 验证的损失值
plt.figure()
plt.subplot(1, 2, 1)
plt.plot(history.history['accuracy'])
# plt.plot(history.history['val_accuracy'])
plt.title('Model accuracy')
plt.ylabel('Accuracy')
plt.xlabel('Epoch')
plt.legend(['Train'], loc='upper left')
# 绘制训练 & 验证的损失值
plt.subplot(1, 2, 2)
plt.plot(history.history['loss'])
# plt.plot(history.history['val_loss'])
plt.title('Model loss')
plt.ylabel('Loss')
plt.xlabel('Epoch')
plt.legend(['Train'], loc='upper left')
plt.show()
