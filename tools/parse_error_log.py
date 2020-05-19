import argparse
import pathlib



def parse_log(error_log):
    SOP_exists = []
    other_errors = []
    for line in error_log:
        if line.startswith('SOP_EXISTS: '):
            split_line = [_parse_path(_) for _ in line[12:].split(' - ')]
            SOP_exists.append(split_line)
        else:
            other_errors.append(line)

    return SOP_exists, other_errors


def _parse_path(path):
    return path[5:-1]


def main():
    args = parse_args()
    error_log = (args.input_dir / 'error.log').read_text().split('\n')
    SOP_exists, other_errors = parse_args(error_log)


def parse_args():
    """Parse input arguments"""
    parser = argparse.ArgumentParser(
        description='Parse error.log written by dicomtosql',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument(
        'input_dir', type=pathlib.Path,
        help="Directory to error.log",
    )
    return parser.parse_args()

if __name__ == '__main__':
    main()