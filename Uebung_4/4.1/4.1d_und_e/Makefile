PROGS=mycp
TEST=testdatei10MB

all: $(PROGS)
copy_test.csv: $(PROGS) $(TEST)
	echo "Puffergroesse, Zeit" > $@;
	for buf in 128 256 512 1024 2048 4096 8192 16384 32768; \
	do \
		echo -n "Copying 10MB with $$buf byte buffersize ..."; \
		copytime=`(time -p ./mycp $(TEST) junk $$buf) 2<&1 | grep real | awk '{print $$2}'`; \
		echo " took: $$copytime seconds"; \
		echo "$$buf, $$copytime" >> $@; \
	done
	rm junk
	rm $(TEST) 
	echo "Results are in $@"
	cat $@

#Erzeuge eine 10MB grosse Testdatei
$(TEST):
	dd if=/dev/urandom of=$@ bs=1M count=10

clean:
	rm -f $(PROGS) ~$(TEST)
