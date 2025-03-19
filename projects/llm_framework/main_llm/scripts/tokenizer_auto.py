from transformers import logging as transformers_logging
transformers_logging.set_verbosity_error()
from transformers import AutoTokenizer, PreTrainedTokenizerFast
import json
import argparse
import sys

def send_msg(content, chunk_size=1024):
    data = [content[i:i + chunk_size] for i in range(0, len(content), chunk_size)]
    for d in data[:-1]:
        sys.stdout.write(d+'rpccontinue\n')
        sys.stdout.flush()
    sys.stdout.write(data[-1]+'\n')
    sys.stdout.flush()


args = argparse.ArgumentParser()
args.add_argument('--model_id', type=str, default='qwen2.5_coder_tokenizer')
args.add_argument('--content', type=str, default='You are Qwen, created by Alibaba Cloud. You are a helpful assistant.')
args = args.parse_args()
tokenizer = AutoTokenizer.from_pretrained(args.model_id)
JRpcResultObj = {
    'jsonrpc': "2.0",
    'result': {"bos_id": tokenizer.bos_token_id, "eos_id": tokenizer.eos_token_id},
    'id': 0
}
send_msg(json.dumps(JRpcResultObj))

line = ''
for part in sys.stdin:
    if part.endswith('rpccontinue\n'):
        line += part[:-12]
        continue
    else:
        line += part
    try:
        JRpcObj = json.loads(line)
        RpcMethod = getattr(tokenizer, JRpcObj['method'])
        if callable(RpcMethod):
            result = RpcMethod(*JRpcObj['params'][0],**JRpcObj['params'][1])
        else:
            result = RpcMethod
        JRpcResultObj = {
            'jsonrpc': JRpcObj['jsonrpc'],
            'result': result,
            'id': JRpcObj['id']
        }
        send_msg(json.dumps(JRpcResultObj))
    except Exception as e:
        JRpcResultObj = {
            'jsonrpc': "2.0",
            'error': {
                'code': -1,
                'message': str(e)
            },
            'id': None
        }
        send_msg(json.dumps(JRpcResultObj))
    line = ''