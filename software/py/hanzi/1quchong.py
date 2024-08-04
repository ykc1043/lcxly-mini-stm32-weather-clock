import re
import glob

def extract_chinese_characters(file_path_pattern):
    all_characters = []
    # 获取所有符合条件的文件
    for file_path in glob.glob(file_path_pattern):
        with open(file_path, 'r', encoding='utf-8') as file:
            content = file.read().replace('\n', '')  # 去掉换行符
            # 提取汉字
            chinese_characters = re.findall(r'[\u4e00-\u9fff]', content)
            all_characters.extend(chinese_characters)
    
    # 去重
    unique_characters = list(set(all_characters))
    
    return unique_characters

def write_to_file(characters, output_file_path):
    with open(output_file_path, 'w', encoding='utf-8') as file:
        for character in characters:
            file.write(character)  # 不添加换行符

if __name__ == "__main__":
    input_file_pattern = 'in*.txt'
    output_file = 'out.txt'
    
    chinese_characters = extract_chinese_characters(input_file_pattern)
    write_to_file(chinese_characters, output_file)

    print("处理完成，结果已写入out.txt")
