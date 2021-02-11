#!/usr/bin/env python3

import sys
import yaml
import jinja2

if __name__ == '__main__':
    try:
        with open(sys.argv[1], 'r') as f:
            rawValues = f.read()
        values = yaml.safe_load(rawValues)
        rawTemplate = sys.stdin.read()
        template = jinja2.Template(rawTemplate)
        sys.stdout.write(template.render(values))
    except IndexError:
        sys.stdout.write("usage: {} [YAML_FILE]\n".format(sys.argv[0]))
        exit(1)
    except FileNotFoundError as e:
        sys.stderr.write("file not found \"{}\"\n".format(sys.argv[1]))
        exit(2)    
