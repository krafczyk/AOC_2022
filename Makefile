all: rust cpp

rust:
	cargo build --manifest-path=rust_src/Cargo.toml

cpp:
	$(MAKE) -C cpp_src
