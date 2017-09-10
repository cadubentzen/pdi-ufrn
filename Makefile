readme:
	cp README.md.in README.md
	perl -pe 's/\{\{(.*)\}\}/`cat $$1`/ge' -i README.md

clean:
	-rm -f README.md
