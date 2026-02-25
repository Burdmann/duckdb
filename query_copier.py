text = []
for line in open(0):
    text.append(line)

text = "".join(text)
queries = text.split(';')
for query in queries:
    for i in range(100):
        print(f"{query};")