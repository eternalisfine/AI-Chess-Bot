from flask import Flask, request, jsonify, send_from_directory
import subprocess
import threading
import os
import json

# Force Flask to look in the current directory for everything
app = Flask(__name__, static_folder='.', static_url_path='')

# Absolute path to your binary
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
ENGINE_PATH = os.path.join(BASE_DIR, "chess_engine")

engine_proc = None
engine_lock = threading.Lock()

def start_engine():
    global engine_proc
    if engine_proc is None or engine_proc.poll() is not None:
        print(f"Starting engine at: {ENGINE_PATH}")
        try:
            engine_proc = subprocess.Popen(
                [ENGINE_PATH],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                text=True,
                bufsize=1
            )
        except Exception as e:
            print(f"ERROR: Could not start binary: {e}")
    return engine_proc

def send_cmd(cmd: str) -> dict:
    with engine_lock:
        proc = start_engine()
        if not proc: return {"error": "Engine binary not found"}
        
        proc.stdin.write(cmd + '\n')
        proc.stdin.flush()
        
        # Read lines until we find one that looks like JSON
        while True:
            line = proc.stdout.readline().strip()
            if not line:
                return {"error": "Engine closed the connection"}
            if line.startswith('{'):
                try:
                    return json.loads(line)
                except:
                    continue # Keep looking

@app.route('/')
def index():
    # Serves index.html from the root folder
    return send_from_directory('.', 'index.html')

@app.route('/api/init', methods=['GET', 'POST'])
def init_game():
    return jsonify(send_cmd('INIT'))

@app.route('/api/legal/<square>', methods=['GET'])
def get_legal(square):
    return jsonify(send_cmd(f'LEGAL {square}'))

@app.route('/api/move', methods=['POST'])
def make_move():
    mv = request.json.get('move', '')
    return jsonify(send_cmd(f'MOVE {mv}'))

@app.route('/api/botmove', methods=['POST'])
def bot_move():
    model = request.json.get('model', 'llama3.2:1b')
    return jsonify(send_cmd(f'BOTMOVE {model}'))

if __name__ == '__main__':
    # chmod +x check
    if os.path.exists(ENGINE_PATH):
        os.chmod(ENGINE_PATH, 0o755)
    app.run(debug=True, port=5000)