from saccr.cli import run_example

if __name__ == "__main__":
    df = run_example(n_paths=5000, steps=24, plot=True)
    print(df.tail())
