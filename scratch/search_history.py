import json

log_path = r"C:\Users\Fadi\.gemini\antigravity\brain\009bb570-94c9-4932-9f62-5f9511a4731f\.system_generated\logs\transcript.jsonl"

with open(log_path, 'r', encoding='utf-8') as f:
    for line in f:
        try:
            obj = json.loads(line)
            content = obj.get("content", "")
            if "IsEntityFriendly" in content and obj.get("step_index", 0) < 150:
                print(f"--- Step {obj.get('step_index')} (Source: {obj.get('source')}, Type: {obj.get('type')}) ---")
                lines = content.split('\n')
                for l in lines[:40]:
                    print(l)
                print("="*60)
        except Exception as e:
            pass
