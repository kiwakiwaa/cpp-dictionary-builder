SUBDIRS = src tools
EXTRA_DIST = GPL LGPL BSD swig

model_topdir = model
corpus = @CORPUS@
dict = @DICT@
modeldir = ${model_topdir}/${corpus}
CLEANFILES = ${modeldir}/*
JAGGER = src/jagger$(EXEEXT)
TRAIN_JAGGER = src/train_jagger$(EXEEXT)

install-data-hook:
	@echo "Installing ${corpus} model.."
	if test -e ${modeldir}/patterns; then \
		$(INSTALL) -d $(DESTDIR)$(pkglibdir)/${modeldir}; \
		$(INSTALL) -p ${modeldir}/* $(DESTDIR)$(pkglibdir)/${modeldir}; \
	fi
	@echo "done."
	@if ! test -e ${modeldir}/patterns; then \
		echo 'WARNING: No model has been built; try make model.'; \
	fi

dist-hook:
	cp $(srcdir)/tools/*.py $(distdir)/tools

uninstall-local:
	if test -e $(DESTDIR)$(pkglibdir)/${modeldir}; then \
		$(RM) $(DESTDIR)$(pkglibdir)/${modeldir}/*; \
	fi

kyoto-benchmark:
	$(MKDIR_P) ${model_topdir}/kyoto
	$(RM) ${model_topdir}/kyoto/*
	@test -e "${dict}" || (echo 'mecab-jumandic not found (JUMAN_DIC_DIR=${dict})' && exit 1)
	@test -e "$(KYOTO_CORPUS_DIR)" || (echo 'Kyoto University Text Corpus not found (KYOTO_CORPUS_DIR=$(KYOTO_CORPUS_DIR))' && exit 1)
	@echo "Converting corpus into JAG format.."
	cat $(KYOTO_CORPUS_DIR)/dat/syn/*.knp | @JUMAN_CONV@ ${model_topdir}/kyoto/train.JAG ${model_topdir}/kyoto/dev.JAG ${model_topdir}/kyoto/test.JAG $(KYOTO_CORPUS_DIR)/id/full/dev.id $(KYOTO_CORPUS_DIR)/id/full/test.id
	@MECAB_CONV@ ${model_topdir}/kyoto/train.JAG > ${model_topdir}/kyoto/train
	@MECAB_CONV@ ${model_topdir}/kyoto/dev.JAG > ${model_topdir}/kyoto/dev
	@MECAB_CONV@ ${model_topdir}/kyoto/test.JAG > ${model_topdir}/kyoto/test
	@echo "done."

kyoto:
	$(MKDIR_P) ${model_topdir}/kyoto
	$(RM) ${model_topdir}/kyoto/*
	@test -e "${dict}" || (echo 'mecab-jumandic not found (JUMAN_DIC_DIR=${dict})' && exit 1)
	@test -e "$(KYOTO_CORPUS_DIR)" || (echo 'Kyoto University Text Corpus not found (KYOTO_CORPUS_DIR=$(KYOTO_CORPUS_DIR))' && exit 1)
	@echo "Converting corpus into JAGGER format.."
	cat $(KYOTO_CORPUS_DIR)/dat/syn/*.knp | @JUMAN_CONV@ ${model_topdir}/kyoto/train.JAG
	@MECAB_CONV@ ${model_topdir}/kyoto/train.JAG > ${model_topdir}/kyoto/train
	@echo "done."

kwdlc-benchmark:
	$(MKDIR_P) ${model_topdir}/kwdlc
	$(RM) ${model_topdir}/kwdlc/*
	@test -e "${dict}" || (echo 'mecab-jumandic not found (JUMAN_DIC_DIR=${dict})' && exit 1)
	cat $(KWDLC_CORPUS_DIR)/knp/*/*.knp | @JUMAN_CONV@ ${model_topdir}/kwdlc/train.JAG ${model_topdir}/kwdlc/dev.JAG ${model_topdir}/kwdlc/test.JAG $(KWDLC_CORPUS_DIR)/id/split_for_pas/dev.id $(KWDLC_CORPUS_DIR)/id/split_for_pas/test.id
	@MECAB_CONV@ ${model_topdir}/kwdlc/train.JAG > ${model_topdir}/kwdlc/train
	@MECAB_CONV@ ${model_topdir}/kwdlc/dev.JAG > ${model_topdir}/kwdlc/dev
	@MECAB_CONV@ ${model_topdir}/kwdlc/test.JAG > ${model_topdir}/kwdlc/test
	@echo "Converting corpus into JAGGER format.."

kwdlc:
	$(MKDIR_P) ${model_topdir}/kwdlc
	$(RM) ${model_topdir}/kwdlc/*
	@test -e "${dict}" || (echo 'mecab-jumandic not found (JUMAN_DIC_DIR=${dict})' && exit 1)
	cat $(KWDLC_CORPUS_DIR)/knp/*/*.knp | @JUMAN_CONV@ ${model_topdir}/kwdlc/train.JAG
	@MECAB_CONV@ ${model_topdir}/kwdlc/train.JAG > ${model_topdir}/kwdlc/train
	@echo "Converting corpus into JAG format.."

kyoto+kwdlc-benchmark: kyoto-benchmark kwdlc-benchmark
	$(MKDIR_P) ${model_topdir}/kyoto+kwdlc
	$(RM) ${model_topdir}/kyoto+kwdlc/*
	cat ${model_topdir}/kyoto/train.JAG ${model_topdir}/kwdlc/train.JAG > ${model_topdir}/kyoto+kwdlc/train.JAG
	cat ${model_topdir}/kyoto/test.JAG ${model_topdir}/kwdlc/test.JAG > ${model_topdir}/kyoto+kwdlc/test.JAG
	@MECAB_CONV@ ${model_topdir}/kyoto+kwdlc/train.JAG > ${model_topdir}/kyoto+kwdlc/train
	@MECAB_CONV@ ${model_topdir}/kyoto+kwdlc/test.JAG > ${model_topdir}/kyoto+kwdlc/test
	@echo "done."

kyoto+kwdlc: kyoto kwdlc
	$(MKDIR_P) ${model_topdir}/kyoto+kwdlc
	$(RM) ${model_topdir}/kyoto+kwdlc/*
	cat ${model_topdir}/kyoto/train.JAG ${model_topdir}/kwdlc/train.JAG > ${model_topdir}/kyoto+kwdlc/train.JAG
	@MECAB_CONV@ ${model_topdir}/kyoto+kwdlc/train.JAG > ${model_topdir}/kyoto+kwdlc/train

${modeldir}/patterns: model

model-benchmark: all ${corpus}-benchmark
	@echo "Training and testing Jagger.."
#	find ${dict} -name "*.csv" | sort | xargs iconv -f EUC-JP -t UTF-8 > ${modeldir}/dict # for jumandic5.1
	find ${dict} -name "*.csv" | sort | xargs cat > ${modeldir}/dict
	${TRAIN_JAGGER} -m ${modeldir} -d ${modeldir}/dict ${modeldir}/train.JAG
	${JAGGER} -m ${modeldir} < ${modeldir}/test > ${modeldir}/test.tmp
	$(PYTHON3) tools/eval.py ${modeldir}/test.tmp ${modeldir}/test.JAG -l ${NUM_POS_FIELD}
	@${RM} ${modeldir}/test.tmp
	@echo "done."

model: all ${corpus}
	@echo "Training Jagger.."
	find ${dict} -name "*.csv" | sort | xargs cat > ${modeldir}/dict
	${TRAIN_JAGGER} -m ${modeldir} -d ${modeldir}/dict ${modeldir}/train.JAG
	@echo "done."

.PHONY: model kyoto kwdlc kyoto+kwdlc
