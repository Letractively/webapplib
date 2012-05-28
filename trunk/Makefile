# /// \file Makefile 
# /// Web Application Library Makefile

################################################################################
# ʹ�÷���

# ʹ�� make ����������ɿ��ļ���
# ʹ�� make install ������ļ�����ͷ�ļ����Ƶ�ϵͳĿ¼��
# ʹ�� make uninstall ����ɾ��ϵͳĿ¼�еĿ��ļ�����ͷ�ļ���
# ʹ�� make clean ���������ǰĿ¼�ı���������ʱ�ļ���
# ʹ�ö�̬��ʱ��Ĭ������£�����ִ�� make install �����ļ���װ��ϵͳĿ¼��
# ����Ŀ��ʹ�� webapplib��Makefile ���ݿɲο� Makefile.example��
# ���߱� root Ȩ�޵��û����Խ��� Makefile �� $(LIBPATH) ��Ϊ����Ŀ¼��
# ������Ŀ¼��ӵ�ִ���ļ����л����Ļ������� LD_LIBRARY_PATH ��.

################################################################################
# ��ǰWEB������汾�� $(WEBAPPLIB_VERSION)
WEBAPPLIB_VERSION = 1.0

# C++ ����������
CXX = g++

# ����ѡ��
CXXFLAGS = -Wall -O2 -fPIC #-s
# -Wall ��ʾ���о�����Ϣ
# -O2 �����Ż�
# -fPIC �����ַ�޹ش��룬�������ɶ�̬���ӿ�
# -s ȥ�����Է�����Ϣ����С�����ļ��ߴ�

################################################################################
# ϵͳͷ�ļ�Ŀ¼
INCPATH = /usr/local/include/webapplib
# ϵͳ���ļ�Ŀ¼
LIBPATH = /usr/local/lib
SYSLIB = /usr/lib

################################################################################
# �Ƿ���� MysqlClient���������� MysqlClient ��ע�ͱ�����
MYSQL = yes
# MySQL ͷ�ļ�·��
MYSQLINC = -I/usr/include/mysql
# MySQL ���ļ�·�������Ӳ���
MYSQLLIB = -L/usr/lib/mysql -lmysqlclient -lm -lz

################################################################################
# ����������ļ��б�
LIBS = String Encode Cgi FileSystem DateTime Template HttpClient TextFile ConfigFile Utility

# �Ƿ����MysqlClient���
ifdef MYSQL
LIBS += MysqlClient
else
CXXFLAGS += -D_WEBAPPLIB_NOMYSQL
MYSQLINC :=
MYSQLLIB :=
endif

OBJS = $(foreach n,$(LIBS),wa$(n).o)
	
# ������ͷ�ļ��б�
WEBAPPINC = webapplib.h $(OBJS:%.o=%.h)
# �����⾲̬���ļ���
WEBAPPLIB = libwebapp.a.$(WEBAPPLIB_VERSION)
# �����⶯̬���ӿ��ļ���
WEBAPPDLL = libwebapp.so.$(WEBAPPLIB_VERSION)

################################################################################
# ����Ŀ��
all: $(WEBAPPLIB) $(WEBAPPDLL)

# ���뿪��������ļ�
$(OBJS): %.o: %.cpp %.h
	@echo ""
	@echo "Compile $(@:%.o=%.cpp) ..."
	$(CXX) $(CXXFLAGS) -c $(@:%.o=%.cpp) $(MYSQLINC)

# ���ɾ�̬���ļ�
$(WEBAPPLIB): $(OBJS)
	@echo ""
	@echo "Build $(WEBAPPLIB) ..."
	$(AR) rc $@ $(OBJS)

# ���ɶ�̬���ļ�
$(WEBAPPDLL): $(OBJS)
	@echo ""
	@echo "Build $(WEBAPPDLL) ..."
	$(CXX) $(CXXFLAGS) -shared -o $@ $(OBJS)
	@echo ""
	@echo "Type \"make install\" to install webapplib"
	@echo "Type \"make uninstall\" to uninstall webapplib"
	@echo "Type \"make -f Makefile.example\" to build example"
	@echo ""

################################################################################
# ִ�а�װ
install:
	@echo ""
	@echo "Install webapplib ..."
	@echo ""
	mkdir -p $(INCPATH)
	chmod 777 $(INCPATH)
	cp -f $(WEBAPPINC) $(INCPATH)
	cp -f $(WEBAPPLIB) $(WEBAPPDLL) $(LIBPATH)
	ln -fs $(LIBPATH)/$(WEBAPPLIB) $(LIBPATH)/libwebapp.a
	ln -fs $(LIBPATH)/$(WEBAPPDLL) $(LIBPATH)/libwebapp.so
	ln -fs $(LIBPATH)/$(WEBAPPLIB) $(SYSLIB)/libwebapp.a
	ln -fs $(LIBPATH)/$(WEBAPPDLL) $(SYSLIB)/libwebapp.so

# ִ��ɾ��
uninstall:
	@echo ""
	@echo "Uninstall webapplib ..."
	@echo ""
	for each in $(WEBAPPINC); \
	do \
		rm -f $(INCPATH)/$$each; \
	done;

	rm -f $(LIBPATH)/$(WEBAPPLIB)
	rm -f $(LIBPATH)/$(WEBAPPDLL)
	unlink $(LIBPATH)/libwebapp.a
	unlink $(LIBPATH)/libwebapp.so
	unlink $(SYSLIB)/libwebapp.a
	unlink $(SYSLIB)/libwebapp.so	
	
# ��ձ�����
clean:
	@echo ""
	@echo "Clean webapplib ..."
	@echo ""
	rm -f $(OBJS) $(WEBAPPLIB) $(WEBAPPDLL)

