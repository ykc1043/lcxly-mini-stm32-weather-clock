def reformat_text(data):
    lines = data.strip().split('\n')
    
    # 读取字符名称和索引
    header = lines[0].strip()
    characters = [info.split('(')[0].strip() for info in header.split(')')[:-1]]
    
    formatted_data = []
    i = 1  # 从第二行开始处理字模数据
    
    while i < len(lines) - 2:
        char = characters.pop(0)  # 取出字符名称
        
        if i >= len(lines) - 2:
            break
        
        # 处理第一个字模数据
        hex_data_1 = lines[i].strip().replace('0x', '0x').replace(',', ', ')
        formatted_data.append(f"'{char}',")
        formatted_data.append(hex_data_1)
        
        # 处理第二个字模数据
        hex_data_2 = lines[i + 1].strip().replace('0x', '0x').replace(',', ', ')
        formatted_data.append(hex_data_2)
        
        # 添加注释
        comment = lines[i + 2].strip()
        formatted_data.append(f"/*\"{char}\",{comment.split(',')[1].strip()}")
        
        i += 3  # 移动到下一个字符的字模数据
    
    return '\n'.join(formatted_data)

def main():
    input_file = 'zimuin.txt'
    output_file = 'zimuout.txt'
    
    # 读取输入文件
    with open(input_file, 'r', encoding='utf-8') as file:
        data = file.read()
    
    # 转换格式
    formatted_output = reformat_text(data)
    
    # 写入输出文件
    with open(output_file, 'w', encoding='utf-8') as file:
        file.write(formatted_output)

if __name__ == "__main__":
    main()
