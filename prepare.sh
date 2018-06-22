NAME='mcchafrao'

echo "Criando arquivo \"$NAME.zip\" para enviar."
# Cria pasta
mkdir -p $NAME

# Copia os arquivos necessarios
cp -r src $NAME/
cp -r includes/* $NAME/src/
cp LEIA-ME.txt $NAME/
cp Makefile $NAME/src/

# Muda os parametros do Makefile
sed -i -e 's/= src/= ./g' $NAME/src/Makefile
sed -i -e 's/= build/= ..\/build/g' $NAME/src/Makefile
sed -i -e 's/= bin/= ./g' $NAME/src/Makefile
sed -i -e 's/= includes/= ./g' $NAME/src/Makefile

# Removendo o que deve ser removido
sed -i -e 's/-g\s*//g' $NAME/src/Makefile
sed -i -e 's/-Wall\s*//g' $NAME/src/Makefile
sed -i -e 's/ -D DEBUG//g' $NAME/src/Makefile

# Compacta a pasta
zip $NAME -r $NAME/ -q

# Remove a pasta
rm -rf $NAME/

echo "Processo concluido!"