echo "min/max"
diff $1/output_correct.txt $1/output_min_max.txt
echo "cluster_10"
diff $1/output_correct.txt $1/output_cluster_10.txt
echo "bloom_10"
diff $1/output_correct.txt $1/output_bloom_10.txt
echo "dictionary_10"
diff $1/output_correct.txt $1/output_dictionary_10.txt
echo "cluster"
diff $1/output_correct.txt $1/output_cluster.txt
echo "bloom"
diff $1/output_correct.txt $1/output_bloom.txt
echo "dictionary"
diff $1/output_correct.txt $1/output_dictionary.txt
echo "cluster_1000"
diff $1/output_correct.txt $1/output_cluster_1000.txt
echo "bloom_1000"
diff $1/output_correct.txt $1/output_bloom_1000.txt
echo "dictionary_1000"
diff $1/output_correct.txt $1/output_dictionary_1000.txt