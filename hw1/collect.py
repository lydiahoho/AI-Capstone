import os
import pandas as pd

folder_path = "news"

csv_files = []

for file in os.listdir(folder_path):
    if file.endswith(".csv"):
        csv_files.append(os.path.join(folder_path, file))

if not csv_files:
    print("No CSV files found in the folder.")
    exit()

merged_df = pd.DataFrame(columns=["title", "category", "date"])


for file in csv_files:
    df = pd.read_csv(file)
    merged_df = pd.concat([merged_df, df], ignore_index=True)

# del duplicated rows
merged_df = merged_df.drop_duplicates(subset="title")

specified_categories = ["影劇", "體育", "政治", "財經", "健康", "時尚"] # "生活", "地方""社會", "國際"
selected_df = pd.DataFrame(columns=["title", "category", "date"])

for category in specified_categories:
    category_df = merged_df[merged_df["category"] == category]
    if len(category_df) > 600:
        selected_df = pd.concat([selected_df, category_df.sample(n=600, random_state=42)], ignore_index=True)
    else:
        selected_df = pd.concat([selected_df, category_df], ignore_index=True)


selected_df.to_csv("6type.csv", index=False)

print("Category Counts:")
for category in specified_categories:
    count = selected_df[selected_df["category"] == category].shape[0]
    print(f"{category}: {count}")
