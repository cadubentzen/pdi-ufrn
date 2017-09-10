readme:
	cp README.md.in README.md.2
	perl -pe 's/\{\{(.*)\}\}/`cat $$1`/ge' -i README.md.2
	expand -t 4 README.md.2 > README.md
	rm -f README.md.2

clean:
	-rm -f README.md
