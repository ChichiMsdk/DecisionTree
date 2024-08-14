.PHONY: clang
clang:
	@make --no-print-directory -f Makefile.clang $(filter-out $@,$(MAKECMDGOALS))

gcc:
	@make --no-print-directory -f Makefile.gcc $(filter-out $@,$(MAKECMDGOALS))

%:
	@:
