from transformers import logging as transformers_logging
transformers_logging.set_verbosity_error()
from transformers import AutoTokenizer, PreTrainedTokenizerFast
import json
import argparse
import sys
import base64

def replace_base64_in_jrpcobj(jrpc_obj):
    """
    Traverse and replace specific structures in a JRpcObj object.
    If {"type": "bytes.base64", "encode": "bmloYW8K"} is found, replace it with the decoded value "nihao".
    """
    if isinstance(jrpc_obj, dict):
        # If it is a dictionary, check if it meets the condition
        if jrpc_obj.get("type") == "bytes.base64" and "encode" in jrpc_obj:
            try:
                # Attempt to decode and replace
                decoded_value = base64.b64decode(jrpc_obj["encode"])
                return decoded_value  # Replace with the decoded value
            except Exception as e:
                # print(f"Decoding error: {e}")
                return jrpc_obj  # If decoding fails, return the original object
        elif jrpc_obj.get("type") == "str.base64" and "encode" in jrpc_obj:
            try:
                # Attempt to decode and replace
                decoded_value = base64.b64decode(jrpc_obj["encode"]).decode("utf-8")
                return decoded_value  # Replace with the decoded value
            except Exception as e:
                # print(f"Decoding error: {e}")
                return jrpc_obj  # If decoding fails, return the original object
        else:
            # If conditions are not met, recursively process the dictionary's values
            return {
                key: replace_base64_in_jrpcobj(value) for key, value in jrpc_obj.items()
            }
    elif isinstance(jrpc_obj, list):
        # If it is a list, recursively process each element
        return [replace_base64_in_jrpcobj(item) for item in jrpc_obj]
    else:
        # If it is another type, return it directly
        return jrpc_obj


def send_msg(content, chunk_size=1024):
    data = [content[i:i + chunk_size] for i in range(0, len(content), chunk_size)]
    for d in data[:-1]:
        sys.stdout.write(d+'rpccontinue\n')
        sys.stdout.flush()
    sys.stdout.write(data[-1]+'\n')
    sys.stdout.flush()

def str2bool(value):
    if isinstance(value, bool):
        return value
    if value.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif value.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')

args = argparse.ArgumentParser()
args.add_argument('--model_id', type=str, default='qwen2.5_coder_tokenizer')
args.add_argument('--content', type=str, default='You are Qwen, created by Alibaba Cloud. You are a helpful assistant.')
args.add_argument('--trust_remote_code', type=str2bool, nargs='?', const=True, default=None)
args.add_argument('--use_fast', type=str2bool, nargs='?', const=False, default=None)
args = args.parse_args()
if args.trust_remote_code is None or args.use_fast is None:
    tokenizer = AutoTokenizer.from_pretrained(args.model_id)
else:
    tokenizer = AutoTokenizer.from_pretrained(args.model_id, trust_remote_code=args.trust_remote_code, use_fast=args.use_fast)
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
        JRpcObjsrc = json.loads(line)
        JRpcObj = replace_base64_in_jrpcobj(JRpcObjsrc)
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