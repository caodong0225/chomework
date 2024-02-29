from flask import Flask, request
import pickle
from keras.models import load_model
import numpy as np

app = Flask(__name__)


# 通过装饰器绑定路由
@app.route('/data', methods=['GET'])
def data():
    # 加载模型和分词器
    model = load_model("test.h5")
    with open('test.pickle', 'rb') as f:
        tokenizer_string = pickle.load(f)
    # 通过request对象获取请求参数
    if request.method == 'GET':
        param = request.args.get("input", None)
        url = param
        dataset = url.split(",")
        data = dataset[:3]
        n = int(dataset[-1])
        while len(data) < n:
            input_sequence = tokenizer_string.texts_to_sequences([data[-3:]])
            print(input_sequence)
            input_seq_length = 3
            X = np.reshape(input_sequence, (len(input_sequence), input_seq_length, 1))
            res = np.argmax(model.predict(X))
            print(res)
            dict_index = tokenizer_string.word_index
            for i in dict_index.items():
                if i[1] == res:
                    data.append(i[0])  #
                    break
        return "".join(data)


if __name__ == '__main__':
    app.run(host='127.0.0.1', debug=True, port='8999')  # 启动服务
