import json


Corelatedstrings = []
with open('RealCorrelation_ForPredict.txt', 'r') as file:
    for line in file:
        Corelatedstrings.append(line.strip('\n'))

# Read the JSON file
with open('output.json', 'r') as file:
    data = json.load(file)

indices_to_remove = []  # 存储要删除的数据项的索引

for index, item in enumerate(data):
    if item["Close Proximity Count"] == 0 and item["Control Dependency Count"] == 0 and item["Data Dependency Count"] == 0 and item["Has Similar Naming"] == 0 and item["Is Sibling"] == 0:
        indices_to_remove.append(index)

# # 根据索引删除数据项
# for index in sorted(indices_to_remove, reverse=True):
#     del data[index]

for item in data:
    item['Is Related'] = 0
    variable_pair = item["Variable Pair"]
    variables = variable_pair.split(',')
    var0 = variables[0]
    var1 = variables[1]
    # print(var0, var1)
    # 遍历字符串列表中的字符串
    for string in Corelatedstrings:
        words = string.split(', ')
        if var0 in words and var1 in words:
            print(f"{var0}和{var1}出现在同一行中: {string}")
            item['Is Related'] = 1

with open('modified_data.json', 'w') as file:
    json.dump(data, file, indent=4)