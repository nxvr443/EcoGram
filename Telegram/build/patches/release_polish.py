import os

def patch_file(path, old, new):
    if not os.path.exists(path):
        print(f"[SKIP] File not found: {path}")
        return False
    with open(path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    if old not in content:
        print(f"[WARN] Target not found in: {path}")
        return False
    content = content.replace(old, new, 1)
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"[OK] Patched: {path}")
    return True

base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))

# 1. Strip ' DEBUG' suffix from About Box
about_box = os.path.join(base_dir, 'SourceFiles', 'boxes', 'about_box.cpp')
patch_file(
    about_box,
    '#ifdef _DEBUG\n\tresult += " DEBUG";\n#endif',
    '#if 0\n\tresult += " DEBUG";\n#endif'
)

# 2. Disable test auto-open craft box on startup
session_controller = os.path.join(base_dir, 'SourceFiles', 'window', 'window_session_controller.cpp')
patch_file(
    session_controller,
    '#if _DEBUG // TEST: Auto-open craft box on startup',
    '#if 0 // TEST: Auto-open craft box on startup'
)

# 3. Disable slow animation debug hotkeys (Ctrl+F11, Ctrl+F12)
mainwindow = os.path.join(base_dir, 'SourceFiles', 'mainwindow.cpp')
patch_file(
    mainwindow,
    '#ifdef _DEBUG\n\t\tif (static_cast<QKeyEvent*>(e)->modifiers().testFlag(',
    '#if 0\n\t\tif (static_cast<QKeyEvent*>(e)->modifiers().testFlag('
)

print("Release polish patching complete!")
