import json

s = r'{"datatypes": ["RPM_FRONT_LEFT", "RPM_FRONT_RIGHT"]}'

t = json.loads(s)

print(type(t))